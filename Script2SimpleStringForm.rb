begin
print 'Digite o nome do arquivo: '
filename = "L:\\Projetos\\RPGXP\\RGSS Powerful Protection\\Novo\\" + gets.to_s.strip.gsub('.rb', '')

file = File.open("#{filename}.rb",'rb')
lines_a = file.readlines
file.close
(lines_a.size - 1).times{|i|
 lines_a[i].strip!
 lines_a[i] += ';'
}

file = File.open("#{filename};.rb",'wb')
file << lines_a.to_s
file.write(';')
file.close
rescue Exception => error
puts error
gets
end