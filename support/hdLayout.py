
def chs(lba):
	sectors_per_track = 63
	heads_per_cylinder = 16

	c = lba / (heads_per_cylinder * sectors_per_track)
	h = (lba % (heads_per_cylinder * sectors_per_track)) / (sectors_per_track)
	s = (lba % (heads_per_cylinder * sectors_per_track)) % (sectors_per_track) + 1

	return (c, h, s)

def chs_bytes(bytes):
	return chs(bytes / 512) # 512 byte sectors

print chs_bytes(0)
print chs_bytes(512)
print chs_bytes(65536 + 512)
print chs_bytes(1024*1024)
print "64 k is %d sectors" % (65536 / 512)
