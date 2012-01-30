begin
  z = File.open('.\\Game.exe', 'rb').read
  $rgsspp_key = (z[0x8F002].to_s + z[0x8F004].to_s + z[0x8F006].to_s).to_i
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