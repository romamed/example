from django.urls import path

from . import views

urlpatterns = [
    path('', views.login, name='login'),
    path('oauth2/', views.oauth2, name='oauth2'),
    path('oauth2/get_access_token/', views.get_access_token, name='get_access_token'),
]