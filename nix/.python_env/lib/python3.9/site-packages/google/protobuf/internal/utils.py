from __future__ import unicode_literals

"""
This module abstracts all the bytes<-->string conversions so that the python 2
and 3 code everywhere else is similar.  This also has a few simple functions
that deal with the fact that bytes are different between the 2 versions even
when using from __future__ import unicode_literals.  For example:
Python 2:
    b'mybytes'[0] --> b'm' (type str)
Python 3:
    b'mybytes'[0] --> 109 (type int)
So in some places we get an index of a bytes object and we have to make sure
the behaviour is the same in both versions of python so functions here take
care of that.
"""

import sys

try:
    import cmp
except ImportError:
    #No cmp function available, probably Python 3
    def cmp(a, b):
        return (a > b) - (a < b)

def string_to_bytestr(string):
    """
    Convert a string to a bytes object.  This encodes the object as well which
    will typically change ord on each element & change the length (i.e. 1 char
    could become 1/2/3 bytes)
    """
    return string.encode('utf-8')

if sys.version_info >= (3,):
    #some constants that are python2 only
    unicode = str
    long = int
    range = range
    unichr = chr

    def iteritems(d):
        return d.items()

    from io import BytesIO as SimIO

    def string_to_bytes(text):
        """
        Convert a string to a bytes object.  This is a raw conversion
        so that ord() on each element remains unchanged.
        Input type: string
        Output type: bytes
        """
        return bytes([ord(c) for c in text])

    def bytes_to_string(byte_array):
        """
        Inverse of string_to_bytes.
        """
        return ''.join([chr(b) for b in byte_array])

    def string_to_bytestr(string):
        """
        Convert a string to a bytes object.  This encodes the object as well which
        will typically change ord on each element & change the length (i.e. 1 char
        could become 1/2/3 bytes)
        """
        return string.encode('utf-8')

    def bytestr_to_string(bytestr):
        """
        Inverse of string_to_bytestr.
        """
        return bytes([c for c in bytestr]).decode('utf-8')

    def byte_chr(bytes_str):
        """
        This converts a *single* input byte to a bytes object.  Usually used in
        conjuction with b'mybytes'[i].  See module description.
        Input: 2: string/pseudo-bytes 3: int
        Output: bytes
        """
        return bytes([bytes_str])

    def bytestr(val):
        """
        Convert a *single* integer to a bytes object.  Usually used like
        bytestr(int).
        Input: int
        Output: bytes
        """
        return bytes([val])
        
    def bytes_as_num(val):
        """ 
        Python 2:
            b'mybytes'[0] --> b'm' (type str)
        Python 3:
            b'mybytes'[0] --> 109 (type int)
        Given a number, returns it. For Python2, we use ord for the conversion.
        """
        return val
   
    def num_as_byte(val):
        """ 
        Converts an element of a byte string to a byte, effectively an inverse of bytes_as_num 
        """
        return bytes([val])
        
else:
    #some constants that are python2 only
    range = xrange
    unicode = unicode
    long = long
    unichr = unichr

    def iteritems(d):
        return d.iteritems()

    try:
        from cStringIO import StringIO as SimIO
    except:
        from StringIO import StringIO as SimIO

    def string_to_bytes(text):
        """
        See other implementation for notes
        """
        return b"".join([c for c in text])

    def bytes_to_string(byte_array):
        """
        See other implementation for notes
        """
        return ''.join([b for b in byte_array])

    def bytestr_to_string(bytestr):
        """
        See other implementation for notes
        """
        return unicode(bytestr, 'utf-8')

    def byte_chr(bytes_str):
        """
        See other implementation for notes
        """
        return bytes_str

    def bytestr(val):
        """
        See other implementation for notes
        """
        return chr(val)

    def bytes_as_num(val):
        """ 
        Python 2:
            b'mybytes'[0] --> b'm' (type str)
        Python 3:
            b'mybytes'[0] --> 109 (type int)
        Given a number, returns it. For Python2, we use ord for the conversion.
        """
        return ord(val)
        
    def num_as_byte(val):
        """ 
        Converts an element of a byte string to a byte, effectively an inverse of bytes_as_num 
        """
        return val
