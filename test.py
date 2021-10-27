import sys
import gc
import example


print(dir(example))
print(example)

api = example.api()
t = api.test()
#api = None
#t = example.api.test()


print(dir(api))
print(api)

print('---------------1')
t.test(1,2,3,4.0)
t.test(1,2,3,4.0)
t.test(1,2,3,4.0)
t.test(1,2,3,4.0)

print('---------------2')
def f(n): return 'A'*n
k = t.test8(f)
print('end test8')
print(k)
del f

print('---------------3')
f2 = t.test12()
print(f2([2,3,4]))
del f2

print('---------------4')
ta = (99, 1.72, 'abc')
t.test13( ta )
print(ta)
print(ta)
del ta

print('---------------5')
print(t.test14())


print('---------------6')
t.test15(b'\x41\x42\x00\x13\xfe\xff')


print('---------------7')
print(t.test16())


print('---------------8')

#test for crash
t.test4("asdfgh1")
t.test4("asdfgh2")
t.test4("asdfgh3")
t.test4("asdfgh4")
t.test4("asdfgh5")
t.test4("asdfgh6")
t.test4("asdfgh7")
t.test4("asdfgh8")





print('---------------9')

print(t.array5())

print('---------------10')

print(t.array3(10))

print('---------------11')
l = [[1,2,9],[3,4],[4,5]]
t.array4(l)
print(l)
del l

print('---------------12')
a=5
t.mul2(a)
print(a)
del a

print('---------------13')
a=[1,2,3,4]
t.array(a)
print(a)
del a

print('---------------14')
b=['a','b','c','d','e','f']
t.array2(b)
print(b)
del b

print('---------------15')
print(t.test)
t.test(1, 2, 3, 4.5)
t.test2(1.2, 3, 4)
t.test3(1, 2.3)
t.test4("Hello World from python")
t.test6("string for std::string")
print(t.test7())

print('---------------16')
def f(n): return 'A'*n
print(t.test8(f))
del f


print('---------------17')
print(t.div(0,0))
print(hex(t.say()))# pointers are handled as size_t

print('---------------18')
def func(n):
    return 'A'*n
print(t.test8(func))
del func

print('---------------19')
def func2(n):
    print('python function called from cpp!!! 2',n)
t.test9(func2)
del func2

print('---------------20')
t.test10({'a':1.1, 'b':2.2})

print('---------------21')
print(t.test11())

print('---------------22')
f = t.test12() # get callback function from c++
print('f([1,2,3,4])=',f([1,2,3,4]))
del f

print('---------------23')
print(t.test12()([99,87]))

#TODO: test for passing python function to c++ function where python function is t.test for example!!

print('---------------24')
t.test17(0x4141414142424242)
print(hex(t.test18()))


print('---------------25')

f = t.test12()
t.test19(f)
del f



print('---------------26')
print(t.name)


print('---------------27')
try:
    t.name = 5
except Exception as e:
    print('OK',e)
    pass

print('---------------28')
print(t.name)


print('---------------29')
def f(x): return "F"*x

try:
    t.test19(f)
    print('FAIL')
except Exception as e:
    print('OK',e)
    pass
    
del f


print('---------------30')

try:
    t.test17('asdasd')
    print('FAIL')
except Exception as e:
    print('OK',e)
    pass


print('---------------31')

try:
    t.test17(1,2,3,4)
    print('FAIL')
except Exception as e:
    print('OK',e)
    pass

print('---------------32')
try:
    print(t.test12()(['a','b']))
    print('FAIL')
except Exception as e:
    print('OK',e)
    pass


print('---------------32.1')
try:
    api.log.asd
    print('FAIL')
except Exception as e:
    print('OK',e)
    pass
    
    
print('---------------33')
try:
    t.test4(1)
    print('FAIL!')
except Exception as e:
    print('OK',e)
    pass

print('---------------33.1')
def f(l): ''.join([str(i) for i in l]) # missing return

try:
    t.test19(f)
    print('FAIL')
except Exception as e:
    print('OK',e)
    pass
    
del f


print('---------------34')
t.test19(lambda x: ''.join(map(str, x)))


print('---------------35')
class X:
  def __init__(self,c): self.c=c
  def func(self, x): return self.c.join(map(str, x))
xx=X('-?-')

t.test19(xx.func)

del xx
del X

print('---------------36')

try:
  t.test19(map)
  print('FAIL')
except Exception as e:
  print('OK', e)



print('---------------37')

try:
  t.test19(lambda x: Exception('check whats happen'))
  print('FAIL')
except Exception as e:
  print('OK', e)



print('---------------38')

def fff(x):
  asdasdqwe

try:
  t.test19(fff)
  print('FAIL')
except Exception as e:
  print('OK', e)
  
  

print('---------------39')

def fff(x):
  raise Exception('check whats happen')

try:
  t.test19(fff)
  print('FAIL')
except Exception as e:
  print('OK', e)


print('---------------40')
f = t.test12() # get callback function from c++
print(f)
try:
    f([[],2,3,4])
    print('FAIL')
except Exception as e:
    print('OK',e)
del f


print('---------------41')
try:
    t.test10({'a':1.1, []:2.2})
    print('FAIL')
except Exception as e:
    print('OK',e)




print('---------------41.1')
try:
    t.test10({'a':1.1, 2.2:2.2})
    print('FAIL')
except Exception as e:
    print('OK',e)



print('---------------42')
try:
    t.test10({'a':1.1, 'b':[]})
    print('FAIL')
except Exception as e:
    print('OK',e)



print('---------------43')
try:
    t.test10({'a':1.1, 'b':[]})
    print('FAIL')
except Exception as e:
    print('OK',e)





print('---------------44')
try:
    t.test13( (99, [], 'abc') )
    print('FAIL')
except Exception as e:
    print('OK',e)




print('---------------45')
try:
    t.test13( (99, 1.1) )
    print('FAIL')
except Exception as e:
    print('OK',e)



print('---------------46')
x = t.__class__('asdf')
print(x)
del x


print('---------------47')
x = t.__class__('asdf')
print(x)
del x


print('---------------48')
x = t.test20()
print(x)
del x

print('---------------49')
try:
    t.test21()
    print('FAIL')
except Exception as e:
    print('OK', e)
    
    
print('---------------50')
try:
    t.test13( (99, 1.1, Exception) )
    print('FAIL')
except Exception as e:
    print('OK',e)
    
    
    
print('---------------51')
try:
    t.test10({'a':1.1, 'b':[]})
    print('FAIL')
except Exception as e:
    print('OK',e)


print('---------------52')

file = api.file()
print(file)
x = api.__class__('dummy')
print(x)
try:
    file.log = x
    print('FAIL')
except Exception as e:
    print('OK',e)
    
del x
del file


print('---------------53')

file = api.file()
x = api.__class__('dummy')
try:
    file.log = file
    print('FAIL')
except Exception as e:
    print('OK',e)
    
del x
del file


print('---------------54')

file = api.file()
print(file)
x = api.__class__('dummy')
log = file.log
print(log)
file.log = log
del x
del file
log.msg = 'asd'
print(log.msg)
del log



print('---------------55')

file = api.file()

try:
    file.log.log
    print('FAIL')
except Exception as e:
    print('OK',e)

del file





print('---------------55.1')
tcp = api.tcp()
print(tcp)
tcp.open()
tcp.write(b'test1')
print(tcp.log)
log = tcp.log
print(log)
tcp.resetLogObject()
print(log)
print(tcp.log)
tcp.write(b'test2')
print(repr(tcp.log.msg))
print(repr(log.msg))
del log
del tcp




print('---------------56')

log = api.log()
T = log.__class__
class X(T):
  def __init__(self): T.__init__(self, 'ABC', log); self.message = ''; print(self)
  def func(self, a): print(self, type(self)); self.f = a
  def setMsg(self, msg): self.message += msg; print('add',msg)
  def getMsg(self): print('get', self.message); return self.message
x = X()
tcp = api.tcp()
tcp.log = x
tcp.open()
tcp.write(b'asdf')
#should show: add write called
del x
del tcp
del log
del T



print('---------------56.1')
tcp = api.tcp()
tcp.open # here cause some errors!
del tcp

print('---------------57')


log = api.log()
T = log.__class__
class A:
  def setMsg(self, msg): self.message += msg; print('add',msg)

class B:
  def getMsg(self): print('get', self.message); return self.message

class X(A, B, T):
  def __init__(self): T.__init__(self, 'ABC', log); self.message = ''; print(self)
  def func(self, a): print(self, type(self)); self.f = a

x = X()
tcp = api.tcp()
tcp.log = x
tcp.open()
tcp.write(b'asdf')
#should show: add write called
del x
del tcp
del log
del T

print('---------------57.1')
tcp = api.tcp()
tcp.open # here cause some errors!
del tcp

print('---------------58')

log = api.log()
T = log.__class__
class X(T):
  def __init__(self): super().__init__('ABC', log); self.message = ''; print(self)
  def func(self, a): print(self, type(self)); self.f = a
  def setMsg(self, msg): self.message += msg; print('add',msg)
  def getMsg(self): print('get', self.message); return self.message
x = X()
tcp = api.tcp()
tcp.log = x
tcp.open()
tcp.write(b'asdf')
#should show: add write called
del x
del tcp
del log
del T
#this test break somethting with type state!


print('---------------58.1')
tcp = api.tcp()
tcp.open # here cause some errors!
del tcp



#TODO: on purpos for testing del is disabled, enable it after
print('---------------X-2')
del t

print('---------------X-1')
del api


print('---------------X')
print('END')
gc.collect()
print('---------------X+1')

