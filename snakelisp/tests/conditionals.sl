(print = (func (line)
    (file-write stdout (cat line "\n"))))

(if true
    (print "yesnana")
    (print "nonana"))

(if false
    (print "yesnono")
    (print "nonono"))

(cond
    (false (print "weird"))
    (true  (print "hello"))
    (else  (print "woird")))
