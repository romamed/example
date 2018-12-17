from django import template
from django.db import connection

register = template.Library()

@register.inclusion_tag('menu/menu.html', takes_context=True)
def draw_menu(context, tree_id):
    path = context['request'].path[1:-1]
    if path != "":
        tree_id = path

    with connection.cursor() as cursor:
        cursor.execute("SELECT  child.* \
                        FROM    menu_tree as parent, \
                                menu_tree as child \
                        WHERE parent.left_num < child.left_num \
                              AND parent.right_num > child.right_num \
                              AND parent.depth = child.depth - 1 \
                              AND parent.left_num IN (SELECT child.left_num \
                                                FROM menu_tree as parent, \
                                                     menu_tree as child \
                                                WHERE parent.left_num >= child.left_num \
                                                      AND parent.right_num <= child.right_num \
                                                      AND parent.left_num = %s) \
                        ORDER BY child.left_num;", [tree_id])
        list = cursor.fetchall()

    cur_depth = 0
    new_list = []
    for item in list:
        if item[2] > cur_depth:
            cur_depth = item[2]
            new_list.append("(")
        elif item[2] < cur_depth:
            for i in range(0, cur_depth - item[2]):
                new_list.append(")")
            cur_depth = item[2]
        new_list.append((item[0], item[3]))


    return {'list':  new_list}