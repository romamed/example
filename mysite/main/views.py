from django.shortcuts import render, redirect
from django.conf import settings
import requests

def main(request):
    if not request.session.get('mysite', False):
        return redirect('http://derenzer.pythonanywhere.com/login')

    link = 'https://api.vk.com/method/users.get?&access_token=' + settings.ACCESS_TOKEN + '&v=5.85'
    response = requests.post(link).json()
    value = response['response'][0]['first_name'] + ' ' + response['response'][0]['last_name']
    context = {'name': value}

    link = 'https://api.vk.com/method/friends.get?order=random&count=5&fields=photo_50&access_token=' + settings.ACCESS_TOKEN + '&v=5.85'
    response = requests.post(link).json()
    count = response['response']['count']
    list = []
    for i in range(count):
        list.append([
            response['response']['items'][i]['first_name'],
            response['response']['items'][i]['last_name'],
            response['response']['items'][i]['photo_50']
        ])
    context['list'] = list

    return render(request, 'main/main.html', context)

def logout(request):
    del request.session['mysite']
    request.session.modified = True

    return redirect('/login')
