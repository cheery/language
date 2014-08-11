(print = (func (line)
    (file-write stdout (cat line "\n"))))

(print "hello")
