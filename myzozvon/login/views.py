from django.shortcuts import render, redirect
from django.conf import settings
import requests
from django.contrib.auth.models import User
from django.contrib.auth import authenticate

from django.http import HttpResponse
from django.contrib.auth import authenticate

def login(request):
    message = ""
    
    if request.session.get('mysite', False):
        return redirect('main')
    elif request.method == 'POST':
        if 'but3' in request.POST:
            link = 'https://oauth.vk.com/authorize?client_id=' + settings.SOCIAL_AUTH_VK_OAUTH2_KEY +\
                '&display=page&redirect_uri=http://127.0.0.1:8000/login/oauth2/get_access_token&scope=friends&response_type=code&v=5.85'

            return redirect(link)
        else:
            username = request.POST['login']
            password = request.POST['password']
            user = authenticate(username=username, password=password)
        
            if 'but1' in request.POST:
                if user is None:
                    user = User.objects.create_user(username, '', password)
                    user.save()
                    message = "[successful registration]"
                else:
                    message = "[this user is already exist]"
            elif 'but2' in request.POST:
                if user is not None:
                    request.session['mysite'] = True
                    request.session.modified = True
                    return redirect('main')
                else:
                    message = "[this user is not exist]"
    context = {
        'message': message
        }

    return render(request, 'login/login.html', context)

def oauth2(request):
    link = 'https://oauth.vk.com/access_token?client_id=' + settings.SOCIAL_AUTH_VK_OAUTH2_KEY + '&client_secret=' + settings.SOCIAL_AUTH_VK_OAUTH2_SECRET + '&redirect_uri=http://127.0.0.1:8000/login/oauth2/get_access_token&code=' + request.GET.get('code')
    response = requests.post(link).json()
    settings.ACCESS_TOKEN = response['access_token']
    request.session['mysite'] = True
    request.session.modified = True

    return redirect('main')