(print = (func (str)
    (file-write stdout str)))
(println = (func (line)
    (file-write stdout (cat line "\n"))))

(fd = (file-open "demo"))
(buffer = (arraybuffer 4096))

(IDLE = (func (ch)
    (start := stop)
    (cond
        ((eq "(" ch)    (print "("))
        ((eq ")" ch)    (print ")")))
    IDLE))

(string = "")
(start  = 0)
(stop   = 0)
(state  = IDLE)
(while (lt 0 (count = (file-read fd buffer)))
    (i = 0)
    (while (lt i count)
        (ch = (chr (idx buffer i)))
        (state = (state ch))
        (stop = (+ stop 1))
        (i = (+ i 1))))
(println "")

(file-close fd)
