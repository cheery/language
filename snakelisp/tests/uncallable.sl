(uncallable-hook := (func (obj)
    (file-write stdout "an object not callable\n")))

(5 "blaa")
