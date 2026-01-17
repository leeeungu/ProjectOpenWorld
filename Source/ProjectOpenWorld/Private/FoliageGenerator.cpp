// Fill out your copyright notice in the Description page of Project Settings.


#include "FoliageGenerator.h"

// Sets default values
AFoliageGenerator::AFoliageGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFoliageGenerator::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFoliageGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

