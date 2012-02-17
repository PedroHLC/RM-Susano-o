begin
  $rgsspp_key = HEY_PUT_A_SECRET_NUMBER_HERE
  $rgsspp_fn = Win32API.new('.\\Game.exe', 'ByPedroHLC', 'II', 'I')
rescue
  print 'failed to decrypt game'
  exit(33)
end
def load_data(path)
  str = File.open(path, 'rb').read
  fsize = str.size
  psw = (((str.size / 2)**2 + $rgsspp_key + 12)/2).to_s
  $rgsspp_fn.call(str.__id__, psw.__id__)
  return Marshal.load(str)
end