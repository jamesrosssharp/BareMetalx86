sectors_per_track = 0x08
heads_per_cylinder = 0x0a + 1


def chs(lba):
	s = lba % (sectors_per_track) + 1
	h = (lba / sectors_per_track) % heads_per_cylinder;
	c = ((lba / sectors_per_track) / heads_per_cylinder);

	return (c, h, s)

def lba(c, h, s):

	return 512 * (c*sectors_per_track*heads_per_cylinder + h*sectors_per_track + s - 1)

def chs_bytes(bytes):
	return chs(bytes / 512) # 512 byte sectors

print chs_bytes(1024*1024)
print chs_bytes(0xf8000)
print chs_bytes(0x10ca00)
print chs_bytes(0x1a800)

print chs_bytes(0x112200)

print "0x%x" % lba(24,4,6)
