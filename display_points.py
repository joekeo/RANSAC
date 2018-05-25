from __future__ import print_function
import pylab
import sys
import numpy as np 

def clip_line_to_lineseg(a, b, x0, y0, x1, y1):
    '''p0 and p1 are 2d points parameterizing an infinite line,
    x0,y0,x1,y1 are the coordinate of the clipping rectangle
    '''
    p0 = [a[0],a[1],1.]
    p1 = [b[0],b[1],1.]
   
    line = np.cross(p0,p1)   
    nx,ny,nz = line
    
    # if clipping against verticals
    if abs(ny) > abs(nx):
        a = [x0 * ny, -(nz + x0 * nx), ny]
        b = [x1 * ny, -(nz + x1 * nx), ny]
        a = a[0:2]/a[2]
        b = b[0:2]/b[2]
        return a,b

    # otherwise clip against horizontals
    a = [-(nz + y0 * ny), y0 * nx, nx]
    b = [-(nz + y1 * ny), y1 * nx, nx]
    a = a[0:2]/a[2]
    b = b[0:2]/b[2]

    return a,b

f = open(sys.argv[1])

# read in the two points parameterizing the line  
p0 = [float(x) for x in f.readline().split()]
p1 = [float(x) for x in f.readline().split()]


# read in the sample points and their inlier status
inliers = []
outliers = []
points = []
  
for line in f:
    x,y,status = line.split()
    x = float(x)
    y = float(y)
    status = int(status)
    
    if status == 1:
        inliers.append( [x,y] )
    else:
        outliers.append( [x,y] )
    points.append([x,y])
        

# plot the inliers and outliers 
 
pylab.plot( [x for x,y in inliers], [y for x,y in inliers], 'g+' )
pylab.plot( [x for x,y in outliers], [y for x,y in outliers], 'r+' )

# clip the line to the graph bounding box and render
xmin, xmax, ymin, ymax =  pylab.axis()
x1, x2 = clip_line_to_lineseg(p0, p1, xmin, ymin, xmax, ymax)
ret = pylab.plot( [x1[0], x2[0]],  [x1[1], x2[1]], 'b-' )
inf_line_plot = ret[0]

def on_lims_changed(axes):
    xmin, xmax, ymin, ymax =  pylab.axis()
    x1, x2 = clip_line_to_lineseg(p0, p1, xmin, ymin, xmax, ymax)
    inf_line_plot.set_xdata( np.array([x1[0], x2[0]]) ) 
    inf_line_plot.set_ydata( np.array([x1[1], x2[1]]) ) 


ax = pylab.gca() 
ax.callbacks.connect('xlim_changed', on_lims_changed)
ax.callbacks.connect('ylim_changed', on_lims_changed)

pylab.show()
    