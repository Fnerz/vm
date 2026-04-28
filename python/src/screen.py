# most of the pygame logic is written by chat gpt cause lazy.
import pygame


class Screen:
    def __init__(self, cols: int, rows: int, cell_size: int, caption: str = "Grid", vm = None, memory_addr: int = 0):
        self.cols = cols
        self.rows = rows
        self.width, self.height = cols * cell_size, rows * cell_size
        self.cell_size = cell_size
        self.caption = caption
        self.pixels = [[0 for _ in range(self.width)] for _ in range(self.height)]
        self.vm = vm
        self.memory_addr = memory_addr
        self.draw_grid_lines = True


    def setup_pygame_window(self) -> None:
        """Initialize pygame and return the screen Surface for a cols x rows grid."""
        pygame.init()
        
        screen = pygame.display.set_mode((self.width, self.height))
        pygame.display.set_caption(self.caption)
        screen.fill((255, 255, 255))
        pygame.display.flip()
        self.screen = screen
        return

    def render(self):
        """Render grid values as black (value > 0) or white (value <= 0)."""

        grid = []
        total_index = self.memory_addr
        for i in range(self.rows):
            row = []
            for j in range(self.cols):
                row.append(self.vm.memory.get(total_index, 0))
                total_index += 1
            grid.append(row)

        for y, row in enumerate(grid):
            for x, value in enumerate(row):
                color = (0, 0, 0) if value > 0 else (255, 255, 255)
                rect = pygame.Rect(x * self.cell_size, y * self.cell_size, self.cell_size, self.cell_size)
                pygame.draw.rect(self.screen, color, rect)

        if self.draw_grid_lines:
            line_color = (200, 200, 200)  # light gray

        # vertical lines
        for x in range(self.cols + 1):
            pygame.draw.line(
                self.screen,
                line_color,
                (x * self.cell_size, 0),
                (x * self.cell_size, self.height)
            )

        # horizontal lines
        for y in range(self.rows + 1):
            pygame.draw.line(
                self.screen,
                line_color,
                (0, y * self.cell_size),
                (self.width, y * self.cell_size)
            )
        
        pygame.display.flip()