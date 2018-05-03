import pygame
from pygame.locals import *
import time
import random

class Base(object):
    def __init__(self, screen_temp, x, y, image_name):
        self.x = x
        self.y = y
        self.screen = screen_temp
        self.image = pygame.image.load(image_name)

class BasePlane(Base):
    def __init__(self, screen_temp, x, y, image_name):
        Base.__init__(self, screen_temp, x, y, image_name)
        self.bullet_list = []
        self.hit = False
        self.bomb_list = []
        self.__create_images()
        self.image_num = 0
        self.image_index = 0

    def display(self):
        if self.hit == False:
            self.screen.blit(self.image, (self.x , self.y))

        else :
            self.screen.blit(self.bomb_list[self.image_index], (self.x, self.y))
            self.image_num +=1
            if self.image_num == 7:
                self.image_num = 0
                self.image_index +=1
            if self.image_index > 3:
                time.sleep(1)
                exit()
        for bullet in self.bullet_list:
            bullet.display()
            bullet.move()
            if bullet.judge():
                self.bullet_list.remove(bullet)
                
    def __create_images(self):
        self.bomb_list.append(pygame.image.load("./feiji/hero_blowup_n1.png"))
        self.bomb_list.append(pygame.image.load("./feiji/hero_blowup_n2.png"))
        self.bomb_list.append(pygame.image.load("./feiji/hero_blowup_n3.png"))
        self.bomb_list.append(pygame.image.load("./feiji/hero_blowup_n4.png"))

    def bomb(self):
        self.hit = True

class HeroPlane(BasePlane):
    def __init__(self, screen_temp):
        BasePlane.__init__(self, screen_temp, 210, 600 , "./feiji/hero1.png")
                
    def move_left(self):
        self.x -=5
    def move_right(self):
        self.x +=5

    def fire(self):
        self.bullet_list.append(Bullet(self.screen, self.x, self.y))

class EnemyPlane(BasePlane):
    def __init__(self, screen_temp):
        BasePlane.__init__(self, screen_temp, 0, 0 , "./feiji/enemy0.png")
        self.direction = "right"
        
    def move(self):
        if self.direction == "right":
            self.x += 5
        elif self.direction == "left":
            self.x -= 5

        if self.x > 480-50:
            self.direction = "left"
        elif self.x < 0:
            self.direction = "right"

    def fire(self):
        random_num = random.randint(1,100)
        if random_num == 8 or random_num == 20:
            self.bullet_list.append(EnemyBullet(self.screen, self.x, self.y))

class BaseBullet(Base):
    def display(self):
        self.screen.blit(self.image, (self.x, self.y))

class Bullet(BaseBullet):
    def __init__(self, screen_temp, x, y):
        BaseBullet.__init__(self, screen_temp, x+40, y-20, "./feiji/bullet.png")

    def move(self):
        self.y -=20

    def judge(self):
        if self.y < 0:
            return True
        else :
            return False

class EnemyBullet(BaseBullet):
    def __init__(self, screen_temp, x,y):
        BaseBullet.__init__(self, screen_temp, x+25, y+40, "./feiji/bullet1.png")

    def move(self):
        self.y +=5

    def judge(self):
        if self.y > 752:
            return True
        else:
            return False
    
def key_control(hero_temp):

    for event in pygame.event.get():
        if event.type == QUIT:
            print("exit")
            exit()
        elif event.type == KEYDOWN:
            if event.key == K_a or event.key == K_LEFT:
                hero_temp.move_left()
            elif event.key == K_d or event.key == K_RIGHT:
                hero_temp.move_right()
            elif event.key == K_SPACE:
                hero_temp.fire()
            elif event.key == K_b:
                print("b")
                hero_temp.bomb()

def main():

    screen = pygame.display.set_mode((480,752),0,32)
    background = pygame.image.load("./feiji/background.png")
    hero = HeroPlane(screen)
    enemy = EnemyPlane(screen)
    
    while True:
        screen.blit(background, (0,0))
        hero.display()
        enemy.display()
        enemy.move()
        enemy.fire()
        pygame.display.update()
        key_control(hero)
        time.sleep(0.01)

if __name__ == "__main__":
    main()
