#coding=utf-8
import socket
import sys
from multiprocessing import Process
import re

class WSGIServer(object):
    addressFamily = socket.AF_INET
    socketType = socket.SOCK_STREAM
    requestQueueSize = 5

    def __init__(self, server_address):
        self.listenSocket = socket.socket(self.addressFamily,self.socketType)
        self.listenSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR,1)
        self.listenSocket.bind(server_address)
        self.listenSocket.listen(self.requestQueueSize)

    def serveForever(self):
        while True:
            self.clientSocket, client_address = self.listenSocket.accept()
            newClientProcess = Process(target=self.handleRequest)
            newClientProcess.start()
            self.clientSocket.close()

    def handleRequest(self):
        recvData = self.clientSocket.recv(2014)
        requestHeaderLines = recvData.splitlines()
        for line in requestHeaderLines:
            print(line)

        httpRequestMethondLine = requestHeaderLines[0]
        httpRequestMethondLine =  str(httpRequestMethondLine, encoding="utf-8")
        print("=====",httpRequestMethondLine)
        getFileName = re.match("[^/]+(/[^ ]*)",httpRequestMethondLine).group(1)
        print("file name is ===>%s"%getFileName)

        if getFileName == '/':
            getFileName = documentRoot + "/index.html"
        else:
            getFileName = documentRoot + getFileName

        print("file name is ===2>%s"%getFileName)

        try:
            f = open(getFileName, "rb")
        except IOError:
            responseHeaderLines = "HTTP/1.1 404 not found\r\n"
            responseHeaderLines += "\r\n"
            responseBody = "===========sorry, file not found ======="
        else:
            responseHeaderLines = "HTTP/1.1 200 OK\r\n"
            responseHeaderLines += "\r\n"
            responseBody = f.read()
            f.close()
        finally:
            responseHeaderLines = bytes(responseHeaderLines, encoding="utf-8")

            response = responseHeaderLines + responseBody
            self.clientSocket.send(response)
            self.clientSocket.close()

serverAddr = (HOST, PORT) = '', 8888
documentRoot = './html'

def makeServer(serverAddr):
    server = WSGIServer(serverAddr)
    return server

def main():
    httpd = makeServer(serverAddr)
    print('web Server: Serving HTTP on port %d ...\n'%PORT)
    httpd.serveForever()

if __name__ == '__main__':
    main()



