$TEMPIMG = ENV['Temp'] + 'tempimg.dat'
def decrypt_img(path)
  descompiled_img = File.open('./' + path + '.rmi', 'rb').read
  psw = (((descompiled_img.size / 2)**2 + $rgsspp_key + 12)/2).to_s
  $rgsspp_fn.call(descompiled_img.__id__, psw.__id__)
  psw = nil
  descompiled_file = File.open($TEMPIMG, 'wb')
  descompiled_file.write(descompiled_img)
  descompiled_file.close
end