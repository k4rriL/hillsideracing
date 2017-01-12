RM := rm -rf

root := ${PWD}

OBJS += \
./car.o \
./main.o \
./pickup.o \
./screen_game.o \
./screen_menu.o \
./terrain.o 

./%.o: ./%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++11 -I${root}/include/ -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

LIBS := -L${root}/lib -Xlinker -rpath '${root}/lib' 
LIBS2 := -lBox2D -lsfml-audio -lsfml-graphics -lsfml-system -lsfml-window -ltgui


# All Target
all: hillsideracing3

hillsideracing3: $(OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross G++ Linker'
	@echo ${root}
	g++ $(LIBS) -o "hillsideracing3" $(OBJS) $(LIBS2) 
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS) hillsideracing3
	-@echo ' '

.PHONY: all clean dependents
