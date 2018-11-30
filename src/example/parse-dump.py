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

parseBuf = 4*[0]
def parseChar(c):
    global parseState
    global header
    if c != header[parseState]:
        parseBuf[parseState] = c
        last = parseState
        parseState = 0
        return parseBuf[:last]
    else:
        parseBuf[parseState] = c
        parseState += 1
        parseState = parseState % len(header)
        return []


with open(filename, "rb") as f:
    content = f.read()
    f.close()

    i = 0
    highByte = False

    byteBuf = []
    for c in content:
        print(hex(c), end=',')

        for c in parseChar(c):

            if (highByte):
                print("h", end='')
                i = c << 8
                highByte = False
            else:
                i = i | c
                #print("%s," % hex(i), end='')
                print("l", end='')

                print(i)
                i = 0
                highByte = True


    