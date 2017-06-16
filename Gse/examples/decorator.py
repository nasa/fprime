'''
Created on Dec 22, 2014

@author: reder
'''
import functools
import numbers

# Simple function decorator example...

def float_args_and_return(function):
    @functools.wraps(function)
    def wrapper(* args, ** kwargs):
        args = [float(arg) for arg in args] 
        return float(function(* args, ** kwargs)) 
    return wrapper

@float_args_and_return 
def mean( first, second, *rest): 
    numbers = (first, second) + rest 
    return sum(numbers) / len(numbers)


print mean(1, '2.3', '4.5', '6.7')

# Example of a decorator factory function
def statically_typed(*types):
    return_type=None
    def decorator(function):
        @functools.wraps(function)
        def wrapper(* args, ** kwargs):
            if len(args) > len(types):
                raise ValueError("too many arguments")
            elif len(args) < len(types):
                raise ValueError("too few arguments")
            for i, (arg, type_) in enumerate(zip(args, types)): 
                if not isinstance(arg, type_): 
                    raise ValueError("argument {} must be of type {}" 
                                     .format( i, type_. __name__)) 
            result = function(*args, **kwargs) 
            if (return_type is not None and 
                not isinstance(result, return_type)):
                raise ValueError("return value must be of type {}".format(
                                        return_type. __name__)) 
            return result
        return wrapper
    return decorator
#
@statically_typed( str, str) 
def make_tagged( text, tag):
    # return "<{ 0} >{ 1} </{ 0>} >".format( tag, exscape(text))
    print text, tag

@statically_typed( str, int, str)  # Will accept any return type 
def repeat( what, count, separator):
    return (( what + separator) * count)[:-len(separator)]

# Testing with proper types
print make_tagged('Test1','Test2')
print repeat( "what", 10, '@')

# Testing with int as str throws exception
try:
    print repeat( "what", '10', '@')
except ValueError:
    pass

# Simple class decorator and method decorator examples...
# NOTE: This decorator example does not seem to function although it runs???

def is_non_empty_str(name, value):
    if not isinstance(value, str): 
        raise ValueError("{} must be of type str".format(name))
    if not bool(value):
        raise ValueError("{} may not be empty".format(name))


def is_valid_isbn(name, value):
    if not isinstance(value, str):
        raise ValueError("{} must be of type str".format(name))

def is_in_range(minimum = None, maximum = None): 
    assert minimum is not None or maximum is not None 
    def is_in_range( name, value): 
        if not isinstance(value, numbers.Number):
            raise ValueError("{} must be a number".format( name)) 
        if minimum is not None and value < minimum: 
            raise ValueError("{} {} is too small".format( name, value)) 
        if maximum is not None and value > maximum: 
            raise ValueError("{} {} is too big".format( name, value)) 
        return is_in_range


def ensure(name, validate, doc = None): 
    def decorator(Class):
        privateName = "__" + name 
        def getter(self): 
            return getattr(self, privateName)
        def setter(self, value):
            validate(name, value)
            setattr(self, privateName, value) 
        setattr(Class, name, property(getter, setter, doc = doc)) 
        return Class
    return decorator


@ensure("title", is_non_empty_str)
@ensure("isbn", is_valid_isbn) 
@ensure("price", is_in_range( 1, 10000)) 
@ensure("quantity", is_in_range( 0, 1000000)) 
class Book: 
    def __init__( self, title, isbn, price, quantity): 
        self.title = title 
        self.isbn = isbn 
        self.price = price 
        self.quantity = quantity 
    @property 
    def value(self): 
        return self.price * self.quantity

b=Book("Strange decorators","ISBN99999",5000,50000)
print b
print b.title
print "Testing the class decorator...."
b.title = "Strange new decorator"
print b.title
b.title = 0
print b.title

# Another class decorator example
# NOTE: this decorator does not seem to work as expected also
class Ensure:
    def __init__(self, validate, doc=None):
        self.validate = validate
        self.doc = doc


def do_ensure(Class): 
    def make_property(name, attribute): 
        privateName = "__" + name 
        def getter( self): 
            return getattr(self, privateName)
        def setter( self, value): 
            attribute.validate( name, value)
            setattr(self, privateName, value) 
        return property(getter, setter, doc = attribute.doc) 
    for name, attribute in Class. __dict__. items():
        if isinstance(attribute, Ensure):
            setattr(Class, name, make_property(name, attribute)) 
    return Class

@do_ensure
class Book2:
    title = Ensure(is_non_empty_str)
    isbn = Ensure(is_valid_isbn) 
    price = Ensure(is_in_range(1, 10000)) 
    quantity = Ensure(is_in_range(0, 1000000))
    
    def __init__(self, title, isbn, price, quantity):
        self.title = title
        self.isbn = isbn
        self.price = price
        self.quantity = quantity
        
    @property
    def value(self):
        return self.price * self.quantity


b=Book2("Strange decorators","ISBN99999",5000,50000)
print b
print b.title
print Book2.title

print getattr(b,'title')

setattr(b, 'title', 'A new strange title')

print b.title

setattr(b, 'title', '')
















