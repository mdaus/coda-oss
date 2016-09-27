# This file was automatically generated by SWIG (http://www.swig.org).
# Version 3.0.10
#
# Do not make changes to this file unless you know what you are doing--modify
# the SWIG interface file instead.





from sys import version_info as _swig_python_version_info
if _swig_python_version_info >= (2, 7, 0):
    def swig_import_helper():
        import importlib
        pkg = __name__.rpartition('.')[0]
        mname = '.'.join((pkg, '_coda_sys')).lstrip('.')
        try:
            return importlib.import_module(mname)
        except ImportError:
            return importlib.import_module('_coda_sys')
    _coda_sys = swig_import_helper()
    del swig_import_helper
elif _swig_python_version_info >= (2, 6, 0):
    def swig_import_helper():
        from os.path import dirname
        import imp
        fp = None
        try:
            fp, pathname, description = imp.find_module('_coda_sys', [dirname(__file__)])
        except ImportError:
            import _coda_sys
            return _coda_sys
        if fp is not None:
            try:
                _mod = imp.load_module('_coda_sys', fp, pathname, description)
            finally:
                fp.close()
            return _mod
    _coda_sys = swig_import_helper()
    del swig_import_helper
else:
    import _coda_sys
del _swig_python_version_info
try:
    _swig_property = property
except NameError:
    pass  # Python < 2.2 doesn't have 'property'.

try:
    import builtins as __builtin__
except ImportError:
    import __builtin__

def _swig_setattr_nondynamic(self, class_type, name, value, static=1):
    if (name == "thisown"):
        return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'SwigPyObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name, None)
    if method:
        return method(self, value)
    if (not static):
        if _newclass:
            object.__setattr__(self, name, value)
        else:
            self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)


def _swig_setattr(self, class_type, name, value):
    return _swig_setattr_nondynamic(self, class_type, name, value, 0)


def _swig_getattr(self, class_type, name):
    if (name == "thisown"):
        return self.this.own()
    method = class_type.__swig_getmethods__.get(name, None)
    if method:
        return method(self)
    raise AttributeError("'%s' object has no attribute '%s'" % (class_type.__name__, name))


def _swig_repr(self):
    try:
        strthis = "proxy of " + self.this.__repr__()
    except __builtin__.Exception:
        strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

try:
    _object = object
    _newclass = 1
except __builtin__.Exception:
    class _object:
        pass
    _newclass = 0

NativeLayer_func__ = _coda_sys.NativeLayer_func__
SYS_FUNC = _coda_sys.SYS_FUNC

def isBigEndianSystem() -> "bool":
    """isBigEndianSystem() -> bool"""
    return _coda_sys.isBigEndianSystem()

def byteSwap(buffer: 'void *', elemSize: 'unsigned short', numElems: 'size_t') -> "void":
    """byteSwap(void * buffer, unsigned short elemSize, size_t numElems)"""
    return _coda_sys.byteSwap(buffer, elemSize, numElems)

def alignedAlloc(*args) -> "void *":
    """
    alignedAlloc(size_t size, size_t alignment)
    alignedAlloc(size_t size) -> void *
    """
    return _coda_sys.alignedAlloc(*args)

def alignedFree(p: 'void *') -> "void":
    """alignedFree(void * p)"""
    return _coda_sys.alignedFree(p)
# This file is compatible with both classic and new-style classes.

cvar = _coda_sys.cvar
SSE_INSTRUCTION_ALIGNMENT = cvar.SSE_INSTRUCTION_ALIGNMENT

