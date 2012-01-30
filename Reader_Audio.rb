$TEMPAUD = ENV['Temp'] + 'tempaud.dat'
$TEMPSE = ENV['Temp'] + 'tempse.dat'
module Audio
  class << self
    alias game_cript_audio_bgm_play bgm_play
    alias game_cript_audio_bgs_play bgs_play
    alias game_cript_audio_me_play  me_play
    alias game_cript_audio_se_play  se_play
    def bgm_play(name, volume=100, pitch=100)
      return if name == @old_bgm_name
      @old_bgm_name = name
      if name != '' and File.exist?('./' + name + '.rma')
        game_cript_audio_bgm_play('./Audio/Empty.rma', 100, 100)
        descompiled_aud = File.open('./' + name + '.rma', 'rb').read.to_s
        psw = (((descompiled_aud.size / 2)**2 + $rgsspp_key + 12)/2).to_s
        $rgsspp_fn.call(descompiled_aud.__id__, psw.__id__)
        psw = nil
        descompiled_file = File.open($TEMPAUD, 'wb')
        descompiled_file.write(descompiled_aud)
        descompiled_file.close
        name = $TEMPAUD
      end
      result = game_cript_audio_bgm_play(name, volume, pitch)
      File.delete($TEMPAUD) if File.exist?($TEMPAUD)
      return result
    end
    def bgs_play(name, volume=100, pitch=100)
      return if name == @old_bgs_name
      @old_bgs_name = name
      if name != '' and File.exist?('./' + name + '.rma')
        game_cript_audio_bgs_play('./Audio/Empty.rma', 100, 100)
        descompiled_aud = File.open('./' + name + '.rma', 'rb').read.to_s
        psw = (((descompiled_aud.size / 2)**2 + $rgsspp_key + 12)/2).to_s
        $rgsspp_fn.call(descompiled_aud.__id__, psw.__id__)
        psw = nil
        descompiled_file = File.open($TEMPAUD, 'wb')
        descompiled_file.write(descompiled_aud)
        descompiled_file.close
        name = $TEMPAUD
      end
      result = game_cript_audio_bgs_play(name, volume, pitch)
      File.delete($TEMPAUD) if File.exist?($TEMPAUD)
      return result
    end
    def me_play(name, volume=100, pitch=100)
      return if name == @old_me_name
      @old_me_name = name
      if name != '' and File.exist?('./' + name + '.rma')
        game_cript_audio_me_play('./Audio/Empty.rma', 100, 100)
        descompiled_aud = File.open('./' + name + '.rma', 'rb').read.to_s
        psw = (((descompiled_aud.size / 2)**2 + $rgsspp_key + 12)/2).to_s
        $rgsspp_fn.call(descompiled_aud.__id__, psw.__id__)
        psw = nil
        descompiled_file = File.open($TEMPAUD, 'wb')
        descompiled_file.write(descompiled_aud)
        descompiled_file.close
        name = $TEMPAUD
      end
      result = game_cript_audio_me_play(name, volume, pitch)
      File.delete($TEMPAUD) if File.exist?($TEMPAUD)
      return result
    end
    def se_play(name, volume=100, pitch=100)
      if name != '' and File.exist?('./' + name + '.rma')
	return game_cript_audio_se_play($TEMPSE, volume, pitch) if @old_se_name == name
	game_cript_audio_se_play('./Audio/Empty.rma', 100, 100)
        descompiled_aud = File.open('./' + name + '.rma', 'rb').read.to_s
        psw = (((descompiled_aud.size / 2)**2 + $rgsspp_key + 12)/2).to_s
        $rgsspp_fn.call(descompiled_aud.__id__, psw.__id__)
        psw = nil
        descompiled_file = File.open($TEMPSE, 'wb')
        descompiled_file.write(descompiled_aud)
        descompiled_file.close
        name = $TEMPSE
      end
      @old_se_name = name
      result = game_cript_audio_se_play(name, volume, pitch)
      return result
    end
  end
end
