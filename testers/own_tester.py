import threading
import colorama
from telnetlib import Telnet

# Использую программу telnet, посылаю простой запрос(нужно потом добавить
# другие запросы для теста), потом читаю по строке
def telnet(host, port, path):
    tl = Telnet(host, port)
    testRequests = [bytes(f'GET {path} HTTP/1.1\r\nHost: localhost\r\n\r\n', "utf-8"),
                    bytes(  '''HTTP/1.1 200 OK\r\n
                            Server: nginx/1.0.4\r\n
                            Date: Thu, 06 Oct 2011 16:14:01 GMT\r\n
                            Content-Type: text/html\r\n
                            Transfer-Encoding: chunked\r\n
                            Connection: keep-alive\r\n
                            Vary: Accept-Encoding\r\n
                            X-Powered-By: PHP/5.3.6\r\n\r\n
                            23\r\n
                            This is the data in the first chunk\r\n
                            1A\r\n
                            and this is the second one\r\n
                            3\r\n
                            con\r\n
                            8\r\n
                            sequence\r\n
                            0\r\n\r\n''', "utf-8")]
    tl.write(testRequests[compare.testNum])
    response = ""
    while 1:
        try:
            response_line = tl.read_until("\n".encode("ascii"), timeout=0.5).decode("utf-8")
        except:
            response_line = ""
        response += response_line
        if (response_line == ""):
            break
    tl.close()
    return (response)

# Подключаюсь к своему серверу и к nginx, сравниваю ответы
def compare(path, details):
    localResponse = telnet("localhost", 8080, path)
    originalResponse = telnet("localhost", 80, path)
    if (localResponse != originalResponse):
        print(f"{colorama.Fore.RED}Test #{compare.testNum} failed{colorama.Style.RESET_ALL}")
        if (details == 1):
            print(f"{colorama.Fore.BLUE}Original response:\n{colorama.Style.RESET_ALL}", originalResponse)
            print(f"{colorama.Fore.BLUE}Local response:\n{colorama.Style.RESET_ALL}", localResponse)
    else:
        print(f"{colorama.Fore.RED}Test #{compare.testNum} correct{colorama.Style.RESET_ALL}")
    compare.testNum = 1

# 0 - не выводить запросы, 1 - выводить
details = 1
# Список потоков
threads = []
# Количество потоков
thread_num = 1

# Создаю потоки, добавляю в список и запускаю
for i in range(thread_num):
    compare.testNum = 1
    t = threading.Thread(target=compare, args=("/", details,))
    threads.append(t)
    t.start()

# Ожидаю завершения всех потоков
for thread in threads:
    thread.join()