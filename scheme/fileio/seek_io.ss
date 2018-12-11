(import (fileio))
(define arg (command-line))
(define argc (length arg))
#;(unless (>= argc 3) (usageErr "%s file {r<length>|R<length>|w<string>|s<offset>}... \n", (car arg)))
(define fd (c-open (cadr arg) (file-options no-fail)))
(define output:r (lambda (c) (display c)))
(define output:R (lambda (c) (display c)))
#;(unless port (errExit "open"))
(define cmdLineErr (lambda x #f))
(let cont ((cmds (cddr arg)))
  (if (not (null? cmds))
    (begin
      (let ((cmd (car cmds)))
        (case (string-ref cmd 0)
          ((#\r #\R) (let ((func (if (eq? (string-ref cmd 0) #\r) output:r output:R)) (len (string-length cmd)))
                       (let f ((n 1))
                         (if (< n len)
                           (func (read port))
                           (f (+ n 1))))))
          ((#\w) (let ((len (string-length cmd)))
                   (let f ((n 1))
                     (if (< n len)
                       (begin (put-u8 port (char->integer (string-ref cmd n)))
                       (f (+ n 1)))))))
          ((#\s) (let ((n (string->number (substring cmd 1 (string-length cmd)))))
                   (set-binary-port-output-index! port n)
                   (set-binary-port-input-index! port n)))
          (else (cmdLineErr "Argument must start with [rRws]: %n" cmd))))
      (cont (cdr cmds)))))
(close-port port)