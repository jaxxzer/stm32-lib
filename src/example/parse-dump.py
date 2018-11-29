filename = "dump.dump"
header = "ya a"
def headerToInt(s):
    i = 0
    for c in s:
        i = i << 8
        i = i | ord(c)
    return hex(i)

def headerToIntList(s):
    l = []
    for c in s:
        l.append(ord(c))
    return l

#header = headerToInt(header)
header = headerToIntList(header)
print("header:", header)
parseState = 0

def parseChar(c):
    global parseState
    global header
    if c != header[parseState]:
        parseState = 0
        return False
    
    parseState += 1
    parseState = parseState % len(header)
    if not parseState:
        return True
    return False



with open(filename, "rb") as f:
    content = f.read()
    f.close()

    i = 0
    highByte = True
    for c in content:
        if parseChar(c):
            print("header!")
            highByte = True
        else:
            if (highByte):
                i = c << 8
                highByte = False
            else:
                i = i | c
                print("%s," % hex(i), end='')
                i = 0
                highByte = True


    