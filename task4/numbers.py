sum = 0

with open("numbers", "w") as f:
    	for i in range(500):
    		f.write(f"{i+1}\n")
    		f.write(f"-{i+1}\n")
        
