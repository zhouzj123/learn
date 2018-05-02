import socket
from multiprocessing import Process
import re

def handleClient(clientSocket):
    recvData = clientSocket.recv(2014)
    requestHeaderLines = recvData.splitlines()

    for line in requestHeaderLines:
        print(line)

    httpRequestMethondLine = requestHeaderLines[0]
    httpRequestMethondLine = str(httpRequestMethondLine, encoding="utf8")
    getFileName = re.match("[^/]+(/[^ ]*)", httpRequestMethondLine).group(1)
    print("file name is ===>%s"%getFileName)

    if getFileName == '/':
        getFileName = documentRoot + "/index.html"
    else:
        getFileName = documentRoot + getFileName
    print("file name is ===>%s" % getFileName)

    try:
        f = open(getFileName,"rb")
    except IOError:
        responseHeaderLines = "HTTP/1.1 404 not found\r\n"
        responseHeaderLines += "\r\n"
        responseBody = "=============sorry, file not found========"
    else:
        responseHeaderLines = "HTTP/1.1 200 OK\r\n"
        responseHeaderLines += "\r\n"
        responseBody = f.read()
        f.close()
    finally:
       # responseHeaderLines = str(responseHeaderLines, encoding="utf8")
        responseBody = str(responseBody, encoding="utf8")
        print(type(responseHeaderLines))
        print(type(responseBody))
        response = responseHeaderLines + responseBody

        response = bytes(response, encoding="utf8")
        clientSocket.send(response)
        clientSocket.close()

def main():
    serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serverSocket.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
    serverSocket.bind(("", 7788))
    serverSocket.listen(10)

    while True:
        clientSocket, clientAddr = serverSocket.accept()
        clientP = Process(target=handleClient, args=(clientSocket,))
        clientP.start()
        clientSocket.close()

documentRoot = './html'

if __name__ == '__main__':
    main()