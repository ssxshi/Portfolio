#-----------------------------------------------------
# Made by Sushant gautam
#
# [Beat Name Formatter] BNF
#
#----Description:----
# A program which asks for the user's
# 1. producer name (aka an alias)
# 2. instagram username (input without '@')
# 3. asks if there are any other producers
#    with whom the user has collaborated with
# 3a. if there are collabs, the instagram
#     of said individual is asked for
# 3b. continues the loop of asking and
#     adding to the list of collabs
#     until the user's input is
#     anything BUT 'Y'
# 4. prompts for the beat name, bpm, and key
# 5. formats everything together
# 6. copies it to user's clipboard
#    and exits program in 5 sec
#
#----Reason----
# I used to make beats (instrumentals) often
# and found it tedious to have to look back at
# my other beat files as I could never remember
# my naming convention. As I gained more knowledge
# about python, I wanted to make a program
# which could make that tedious task much more simple.
#-----------------------------------------------------


import pyperclip
import time

open = "("
close = ")"
collabList = []

def CheckCollab(areCollabs):
  if str.upper(areCollabs) == "Y" or "":
    AddCollabs()

def AskCollab():
  return str(input("\nAre there (anymore) collabs (and has insta)? y/N\n"))

def AddCollabs():
  collabList.append(str(input("\nEnter collab insta handle:\n")))
  CheckCollab(AskCollab())

prodName = input("Enter Producer name:\n")
instaHandle = input("\nEnter Insta handle:\n")
areCollabs = AskCollab()

CheckCollab(areCollabs)

name = input("\nEnter Beat Name:\n")
bpm = open + input("\nEnter BPM:\n") + " bpm" + close
key = open + input("\nEnter Key:\n") + close

toPrint = prodName + " - " + name + " " + bpm + " " + key + " - @" + instaHandle

if len(collabList) > 0:
  for collab in collabList:
    toPrint = toPrint + " @" + collab

print(toPrint)
pyperclip.copy(toPrint)
print("--Copied to clipboard--")
print("--Closing in 5 seconds--")
for i in range(5, 0, -1):
  print(i)
  time.sleep(1)