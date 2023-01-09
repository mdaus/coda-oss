% https://www.mathworks.com/help/matlab/ref/h5write.html
h5create('123_barfoo_catdog_cx.h5', '/1/bar/cat/i', [10, 1])
mydata = rand(10, 1);
h5write('123_barfoo_catdog_cx.h5', '/1/bar/cat/i', mydata)
h5disp('123_barfoo_catdog_cx.h5')
