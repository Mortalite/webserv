import fcntl, os, colorama
from subprocess import Popen, PIPE
from time import sleep

def writeInProcess(proc, str):
    proc.stdin.write(str.encode())
    proc.stdin.flush()

def netcat(host, port, path):
    proc = Popen([f'nc -v {host} {port}'], shell=True, stdout=PIPE, stdin=PIPE, stderr=PIPE)
    fcntl.fcntl(proc.stdout.fileno(), fcntl.F_SETFL, os.O_NONBLOCK)
    writeInProcess(proc, f'GET {path} HTTP/1.1\n')
    writeInProcess(proc, '\n\n')
    sleep(0.5)
    return (proc.stdout.read().decode())

def compare(host, path, filename):
    localResponse = netcat("localhost", 8080, host + path + filename)
    originalResponse = netcat(host, 80, path + filename)
    if (localResponse != originalResponse):
        print(f"{colorama.Fore.RED}Test #{compare.testNum} failed{colorama.Style.RESET_ALL}")
        print(f"{colorama.Fore.BLUE}Original response:\n{colorama.Style.RESET_ALL}", originalResponse)
        print(f"{colorama.Fore.BLUE}Local response:\n{colorama.Style.RESET_ALL}", localResponse)
    else:
        print(f"{colorama.Fore.RED}Test #{compare.testNum} correct{colorama.Style.RESET_ALL}")
    compare.testNum += 1

compare.testNum = 0
compare("google.com", "/", "")