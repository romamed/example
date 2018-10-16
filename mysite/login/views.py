from django.shortcuts import render, redirect
from django.conf import settings
import requests

def login(request):
    if request.session.get('mysite', False):
        return redirect('http://derenzer.pythonanywhere.com')

    return render(request, 'login/login.html')

def oauth2(request):
    link = 'https://oauth.vk.com/authorize?client_id=' + settings.SOCIAL_AUTH_VK_OAUTH2_KEY + '&display=page&redirect_uri=http://derenzer.pythonanywhere.com/login/oauth2/get_access_token&scope=friends&response_type=code&v=5.85'

    return redirect(link)

def get_access_token(request):
    link = 'https://oauth.vk.com/access_token?client_id=' + settings.SOCIAL_AUTH_VK_OAUTH2_KEY + '&client_secret=' + settings.SOCIAL_AUTH_VK_OAUTH2_SECRET + '&redirect_uri=http://derenzer.pythonanywhere.com/login/oauth2/get_access_token&code=' + request.GET.get('code')
    response = requests.post(link).json()
    settings.ACCESS_TOKEN = response['access_token']
    request.session['mysite'] = True
    request.session.modified = True

    return redirect('http://derenzer.pythonanywhere.com')
