from django.shortcuts import render

def main(request, **kwargs):
    return render(request, 'main/main.html')