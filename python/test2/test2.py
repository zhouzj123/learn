import pygame

if __name__ == "__main__":
    screen = pygame.display.set_mode((480,890),0,32)
    background = pygame.image.load("./feiji/background.png").convert()

    while True:
        screen.blit(background,(0,0))
        pygame.display.update()
