import colorama
from telnetlib import Telnet


def telnet(host, port, path):
    tl = Telnet(host, port)
    testRequests = [bytes(f'GET {path} HTTP/1.1\r\nHost: localhost\r\n\r\n', "utf-8")]
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
    compare.testNum += 1

compare.testNum = 0
details = 1
compare("/", details)
