from django.urls import path

from . import views

urlpatterns = [
    path('', views.main, name='main'),
    path('logout/', views.logout, name='logout'),
    path('<band>/', views.band, name='band'),
    path('<band>/<album>/', views.album, name='album'),
]