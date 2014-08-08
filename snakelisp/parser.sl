(print = (func (str)
    (file-write stdout str)))
(println = (func (line)
    (file-write stdout (cat line "\n"))))
(print-token = (func (name string)
    (file-write stdout (cat name " " string "\n"))))

(fd = (file-open "demo"))
(buffer = (arraybuffer 4096))

(IDLE = (func (ch)
    (start := stop)
    (cond
        ((eq "(" ch)    (print-token "lparen" "(") IDLE)
        ((eq ")" ch)    (print-token "rparen" ")") IDLE)
        ((eq "#" ch)    COMMENT)
        ((space? ch)    IDLE)
        ((eq "'" ch)    (string := ch) (STRINGQUOTE ch))
        ((eq "\"" ch)   (string := ch) (STRINGQUOTE ch))
        ((digit? ch)    (string := ch) DIGIT)
        (else           (string := ch) SYMBOL))))

(COMMENT = (func (ch)
    (if (eq ch "\n")
        (IDLE ch)
        COMMENT)))
        
(SYMBOL = (func (ch)
    (cond 
        ((special? ch)  (print-token "symbol" string)  (IDLE ch))
        (else           (string := (cat string ch))     SYMBOL))))

(STRINGQUOTE = (func (terminal)
    (escape = (func (ch) (string := (cat string ch)) same))
    (same = (func (ch)
        (cond
            ((eq terminal ch)
                (string := (cat string ch))
                (print-token "string" string) IDLE)
            ((eq "\\" ch)
                (string := (cat string ch)) escape)
            (else
                (string := (cat string ch)) same))))))

(DIGIT = (func (ch)
    (cond
        ((digit? ch) (string := (cat string ch)) DIGIT)
        ((hex? ch)   (string := (cat string ch)) DIGIT)
        ((eq ch "x") (string := (cat string ch)) DIGIT)
        ((eq ch ".") (string := (cat string ch)) DOUBLE)
        (else        (print-token "number" string) IDLE))))

(DOUBLE = (func (ch)
    (cond
        ((digit? ch) (string := (cat string ch)) DOUBLE)
        (else        (print-token "double" string) IDLE))))

(special? = (func (ch)
    (or
        (space? ch)
        (eq ch "(")
        (eq ch ")"))))

(space? = (func (ch)
    (or
        (eq ch "\n")
        (eq ch " "))))

(digit? = (func (ch) (and (lt "0" ch) (lt ch "9"))))
(hex?   = (func (ch) (or (digit? ch) (and (lt "0" ch) (lt ch "9")))))

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

(file-close fd)
