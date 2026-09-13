import json
import random
from datetime import datetime, timedelta

def generate_huge_log(filename="huge_log.json", num_lines=10_000_000):
    print(f"Generating {num_lines} logs in {filename}...")
    
    levels = ["INFO", "INFO", "INFO", "WARN", "DEBUG", "ERROR"]
    messages = [
        "User authenticated successfully", "Database timeout",
        "Retrying connection to external API", "Cache cleared",
        "Invalid payload format received", "Disk space running low",
        "Background worker started"
    ]
    
    dt = datetime.now()
    
    with open(filename, 'w', encoding='utf-8') as f:
        for i in range(num_lines):
            log = {
                "timestamp": dt.strftime("%Y-%m-%d %H:%M:%S"),
                "level": random.choice(levels),
                "message": f"[{i}] {random.choice(messages)}",
                "metadata": {"ip": f"192.168.1.{random.randint(1,255)}"}
            }
            f.write(json.dumps(log) + '\n')
            
            dt += timedelta(seconds=random.randint(0, 2))
            
            if i % 100000 == 0 and i > 0:
                print(f"Generated {i} lines...")
                
    print("Done!")

if __name__ == "__main__":
    generate_huge_log()