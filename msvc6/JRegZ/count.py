import math
import sys
import os 

def P( n ):
    return n*(n+1) / 2

def f( x, y, a ):
    return P(y) + x
    #return (2*y - x*x - x*(1-2*a)) / 2
    #return P(a) - P(a - x) + y - x


if __name__ == '__main__':
    a = int(sys.argv[1])
    
    print "Matrix size: %d"%a
    
    for j in xrange(0,a):
        for i in xrange(0,j+1):
            id = f( i,j,a )
            print "%3d"%id,
        print
        