FUNCTION fact :
PARAM n
t0 := n
t1 := #1
IF t0 == t1 GOTO label0
GOTO label1
LABEL label0 :
t2 := n
RETURN t2
GOTO label2
LABEL label1 :
t4 := n
t7 := n
t8 := #1
t6 := t7 - t8
ARG t6
t5 := CALL fact
t3 := t4 * t5
RETURN t3
LABEL label2 :
FUNCTION main :
READ t9
m := t9
t10 := m
t11 := #1
IF t10 > t11 GOTO label3
GOTO label4
LABEL label3 :
t13 := m
ARG t13
t12 := CALL fact
result := t12
GOTO label5
LABEL label4 :
t14 := #1
result := t14
LABEL label5 :
t15 := result
WRITE t15
t16 := #0
RETURN t16
