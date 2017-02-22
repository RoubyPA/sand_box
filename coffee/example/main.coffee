console.log "..."

# Var
num1 = 42
num2 = 23

# Function
fun1 = (x,y) ->
  x + y

fun_p_1 = (x,y) ->
  console.log "num_tmp = " + (x + y)

fun2 = (x,y) ->
  x * y

fun3 = (x,y) ->
  x / y


# exec function
num3 = fun1 num1, num2
num4 = fun2 num1, num2
num5 = fun3 num1, num2

fun_p_1 num1, num2

# Print
console.log "num3 = " + num3
console.log "num4 = " + num4
console.log "num5 = " + num5
