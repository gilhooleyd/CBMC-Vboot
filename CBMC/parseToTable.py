
import re

gbb_line = ""
steps = re.compile("size of");
VCC = re.compile("Generated.*VCC");
seconds = re.compile(".*User time");
data = re.compile(".*Maximum resident set size");

with open("output") as f:
    for line in f:
        if (steps.match(line)):
            step = re.search(r'\d+', line).group()
        elif (VCC.match(line)):
            print line
            v = re.search(r'\d+', line).group()
        elif (seconds.match(line)):
            print line
            s = re.search(r'\d+', line).group()
        elif (data.match(line)):
            print line
            d = re.search(r'\d+', line).group()

print ""
print "test & " + step + " & " + str(v) + " & " + str(s) + " & " + str(int(d) / 1024) + " & unsat \\\\ "
