import requests
import time

# --- CONFIGURATION ---
TELEGRAM_TOKEN = "8215508330:AAH89E2yXIslUZDwb3gIkxgeHdEzLnR7EVk"
CHAT_ID = "-1003754800711"
# Jo API aapne Canary se nikali
API_URL = "https://www.cricbuzz.com/api/home" 

# Headers jo aapke screenshot mein the (Very Important)
HEADERS = {
    "host": "www.cricbuzz.com",
    "user-agent": "Mozilla/5.0 (Linux; Android 13; SM-F127G) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/146.0.7680.164 Mobile Safari/537.36",
    "x-requested-with": "mark.via.gp",
    "referer": "https://www.cricbuzz.com/",
    "accept": "application/json"
}

def send_telegram_message(message):
    """Telegram par message bhejne ka function"""
    url = f"https://api.telegram.org/bot{TELEGRAM_TOKEN}/sendMessage"
    payload = {"chat_id": CHAT_ID, "text": message, "parse_mode": "HTML"}
    try:
        requests.post(url, json=payload)
    except Exception as e:
        print(f"Error sending message: {e}")

def get_live_scores():
    """Cricbuzz API se score nikalne ka function"""
    try:
        response = requests.get(API_URL, headers=HEADERS, timeout=10)
        if response.status_code == 200:
            data = response.json()
            
            # Note: API structure change ho sakti hai, 
            # Response tab mein 'matches' ya 'liveMatches' key dhoondein.
            # Yeh ek general logic hai:
            matches_list = data.get('matches', [])
            
            updates = "<b>🏏 Live Match Updates:</b>\n\n"
            
            for match in matches_list[:5]: # Top 5 matches dikhayega
                m_name = match.get('matchName', 'Match')
                status = match.get('status', 'No live update')
                score = match.get('score', '') # Check key in your Canary response
                
                updates += f"🔹 <b>{m_name}</b>\nScore: {score}\nStatus: {status}\n\n"
            
            return updates
        else:
            return f"Error: API status {response.status_code}"
    except Exception as e:
        return f"Fetch Error: {str(e)}"

# --- MAIN LOOP ---
print("Bot started... Press Ctrl+C to stop.")
last_update = ""

while True:
    current_update = get_live_scores()
    
    # Sirf tabhi message bhejega jab score ya status change ho
    if current_update != last_update:
        send_telegram_message(current_update)
        last_update = current_update
        print("Update sent to Telegram!")
    
    # Har 60 seconds mein check karega (Rate limit se bachne ke liye)
    time.sleep(60)
