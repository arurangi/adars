def f(*args, **kwargs):
    for arg in args:
        print(arg)

f(*[100, 23, 4, 342])
