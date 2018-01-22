
#include "HTTPFileServer.h"
#include <Arduino.h>
#include <WebServer.h>
#include <SPIFFS.h>

WebServer server(80);

#define DBG_OUTPUT_PORT Serial

File fsUploadFile;


//format bytes
String formatBytes(size_t bytes)
{
    if (bytes < 1024)
    {
        return String(bytes) + "B";
    }
    else if (bytes < (1024 * 1024))
    {
        return String(bytes / 1024.0) + "KB";
    }
    else if (bytes < (1024 * 1024 * 1024))
    {
        return String(bytes / 1024.0 / 1024.0) + "MB";
    }
    else
    {
        return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
    }
}

String getContentType(String filename)
{
    if (server.hasArg("download"))
        return "application/octet-stream";
    else if (filename.endsWith(".htm"))
        return "text/html";
    else if (filename.endsWith(".html"))
        return "text/html";
    else if (filename.endsWith(".css"))
        return "text/css";
    else if (filename.endsWith(".js"))
        return "application/javascript";
    else if (filename.endsWith(".png"))
        return "image/png";
    else if (filename.endsWith(".gif"))
        return "image/gif";
    else if (filename.endsWith(".jpg"))
        return "image/jpeg";
    else if (filename.endsWith(".ico"))
        return "image/x-icon";
    else if (filename.endsWith(".xml"))
        return "text/xml";
    else if (filename.endsWith(".pdf"))
        return "application/x-pdf";
    else if (filename.endsWith(".zip"))
        return "application/x-zip";
    else if (filename.endsWith(".gz"))
        return "application/x-gzip";
    return "text/plain";
}

bool handleFileRead(String path)
{
    //DBG_OUTPUT_PORT.println("handleFileRead: " + path);
    if (path.endsWith("/"))
        path += "index.htm";
    String contentType = getContentType(path);
    String pathWithGz = path + ".gz";
    if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path))
    {
        if (SPIFFS.exists(pathWithGz))
            path += ".gz";
        File file = SPIFFS.open(path, "r");
        size_t sent = server.streamFile(file, contentType);
        file.close();
        return true;
    }
    return false;
}

void handleFileUpload()
{
    if (server.uri() != "/edit")
        return;
    HTTPUpload &upload = server.upload();
    if (upload.status == UPLOAD_FILE_START)
    {
        String filename = upload.filename;
        if (!filename.startsWith("/"))
            filename = "/" + filename;
        //DBG_OUTPUT_PORT.print("handleFileUpload Name: ");
        //DBG_OUTPUT_PORT.println(filename);
        fsUploadFile = SPIFFS.open(filename, "w");
        filename = String();
    }
    else if (upload.status == UPLOAD_FILE_WRITE)
    {
        //DBG_OUTPUT_PORT.print("handleFileUpload Data: "); DBG_OUTPUT_PORT.println(upload.currentSize);
        if (fsUploadFile)
            fsUploadFile.write(upload.buf, upload.currentSize);
    }
    else if (upload.status == UPLOAD_FILE_END)
    {
        if (fsUploadFile)
            fsUploadFile.close();
        //DBG_OUTPUT_PORT.print("handleFileUpload Size: ");
        //DBG_OUTPUT_PORT.println(upload.totalSize);
    }
}

void handleFileDelete()
{
    if (server.args() == 0)
        return server.send(500, "text/plain", "BAD ARGS");
    String path = server.arg(0);
    //DBG_OUTPUT_PORT.println("handleFileDelete: " + path);
    if (path == "/")
        return server.send(500, "text/plain", "BAD PATH");
    if (!SPIFFS.exists(path))
        return server.send(404, "text/plain", "FileNotFound");
    SPIFFS.remove(path);
    server.send(200, "text/plain", "");
    path = String();
}

void handleFileCreate()
{
    if (server.args() == 0)
        return server.send(500, "text/plain", "BAD ARGS");
    String path = server.arg(0);
    //DBG_OUTPUT_PORT.println("handleFileCreate: " + path);
    if (path == "/")
        return server.send(500, "text/plain", "BAD PATH");
    if (SPIFFS.exists(path))
        return server.send(500, "text/plain", "FILE EXISTS");
    File file = SPIFFS.open(path, "w");
    if (file)
        file.close();
    else
        return server.send(500, "text/plain", "CREATE FAILED");
    server.send(200, "text/plain", "");
    path = String();
}

void returnFail(String msg)
{
    server.send(500, "text/plain", msg + "\r\n");
}


void handleFileList()
{
    if (!server.hasArg("dir"))
    {
        returnFail("BAD ARGS");
        return;
    }
    String path = server.arg("dir");
    if (path != "/" && !SPIFFS.exists((char *)path.c_str()))
    {
        returnFail("BAD PATH");
        return;
    }
    File dir = SPIFFS.open((char *)path.c_str());
    path = String();
    if (!dir.isDirectory())
    {
        dir.close();
        returnFail("NOT DIR");
        return;
    }
    dir.rewindDirectory();

    String output = "[";
    for (int cnt = 0; true; ++cnt)
    {
        File entry = dir.openNextFile();
        if (!entry)
            break;

        if (cnt > 0)
            output += ',';

        output += "{\"type\":\"";
        output += (entry.isDirectory()) ? "dir" : "file";
        output += "\",\"name\":\"";
        // Ignore '/' prefix
        output += entry.name() + 1;
        output += "\"";
        output += "}";
        entry.close();
    }
    output += "]";
    server.send(200, "text/json", output);
    dir.close();
}

void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
    //DBG_OUTPUT_PORT.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if (!root)
    {
        //DBG_OUTPUT_PORT.println("Failed to open directory");
        return;
    }
    if (!root.isDirectory())
    {
       // DBG_OUTPUT_PORT.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
           // DBG_OUTPUT_PORT.print("  DIR : ");
           // DBG_OUTPUT_PORT.println(file.name());
            if (levels)
            {
                listDir(fs, file.name(), levels - 1);
            }
        }
        else
        {
           // DBG_OUTPUT_PORT.print("  FILE: ");
           // DBG_OUTPUT_PORT.print(file.name());
            //DBG_OUTPUT_PORT.print("  SIZE: ");
           // DBG_OUTPUT_PORT.println(file.size());
        }
        file = root.openNextFile();
    }
}


HTTPFileServer::HTTPFileServer(){

}

void HTTPFileServer::Setup()
{
    SPIFFS.begin();
    {
        listDir(SPIFFS, "/", 0);
    }

    //SERVER INIT
    //list directory
    server.on("/list", HTTP_GET, handleFileList);
    //load editor
    server.on("/edit", HTTP_GET, []() {
        if (!handleFileRead("/edit.htm"))
            server.send(404, "text/plain", "FileNotFound");
    });
    //create file
    server.on("/edit", HTTP_PUT, handleFileCreate);
    //delete file
    server.on("/edit", HTTP_DELETE, handleFileDelete);
    //first callback is called after the request has ended with all parsed arguments
    //second callback handles file uploads at that location
    server.on("/edit", HTTP_POST, []() { server.send(200, "text/plain", ""); }, handleFileUpload);

    //called when the url is not defined here
    //use it to load content from SPIFFS
    server.onNotFound([]() {
        if (!handleFileRead(server.uri()))
            server.send(404, "text/plain", "FileNotFound");
    });

    //get heap status, analog input value and all GPIO statuses in one json call
    server.on("/all", HTTP_GET, []() {
        String json = "{";
        json += "\"heap\":" + String(ESP.getFreeHeap());
        json += ", \"analog\":" + String(analogRead(A0));
        json += "}";
        server.send(200, "text/json", json);
        json = String();
    });
    server.begin();
    //DBG_OUTPUT_PORT.println("HTTP server started");
}

void HTTPFileServer::Task()
{
    server.handleClient();
}
