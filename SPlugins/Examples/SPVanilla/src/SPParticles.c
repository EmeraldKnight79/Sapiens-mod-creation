

#include "SPParticles.h"

enum {
	sp_emitterTypeCampfire = 1,
	sp_emitterTypeWoodChop,
	sp_emitterTypeFeathers
};

enum {
	sp_particleRenderTypeSmoke = 1,
	sp_particleRenderTypeFire,
	sp_particleRenderTypeStandard,
	sp_particleRenderTypeSpark,
};


bool spEmitterWasAdded(SPParticleThreadState* threadState,
	SPParticleEmitterState* emitterState)
{
	bool removeImmediately = false;
	SPRand* spRand = threadState->spRand;


	switch(emitterState->type)
	{
	case sp_emitterTypeCampfire:
	{
	
	}

		break;
	case sp_emitterTypeWoodChop:
	{
		removeImmediately = true;
		double posLength = spVec3Length(emitterState->p);
		SPVec3 normalizedPos = spVec3Div(emitterState->p, posLength);
		SPVec3 gravity = spVec3Mul(normalizedPos, SP_METERS_TO_PRERENDER(-10.0));


		for(int i = 0; i < 32; i++)
		{
			SPParticleState state;
			SPVec3 randPosVec = spVec3Mul(spRandGetVec3(spRand), SP_METERS_TO_PRERENDER(0.2));
			SPVec3 randVelVec = spRandGetVec3(spRand);
			state.p = spVec3Add(spVec3Mul(normalizedPos, posLength), randPosVec);
			state.v = spVec3Mul(spVec3Add(normalizedPos, randVelVec), SP_METERS_TO_PRERENDER(1.0));
			state.particleTextureType = 3;
			state.lifeLeft = 1.0;
			state.randomValueA = spRandGetValue(spRand);
			state.gravity = gravity;

			/*(*addParticle)(particleManager,
				sp_particleRenderTypeStandard,
				&state);*/
		}
	}
		break;
	case sp_emitterTypeFeathers:
	{
		removeImmediately = true;
		double posLength = spVec3Length(emitterState->p);
		SPVec3 normalizedPos = spVec3Div(emitterState->p, posLength);
		SPVec3 gravity = spVec3Mul(normalizedPos, SP_METERS_TO_PRERENDER(-2.0));

		for(int i = 0; i < 8; i++)
		{
			SPParticleState state;
			SPVec3 randPosVec = spVec3Mul(spRandGetVec3(spRand), SP_METERS_TO_PRERENDER(0.2));
			SPVec3 randVelVec = spRandGetVec3(spRand);
			state.p = spVec3Add(spVec3Mul(normalizedPos, posLength + SP_METERS_TO_PRERENDER(0.25)), randPosVec);
			state.v = spVec3Mul(spVec3Add(normalizedPos, randVelVec), SP_METERS_TO_PRERENDER(1.0));
			state.particleTextureType = 4;
			state.lifeLeft = 1.0;
			state.randomValueA = spRandGetValue(spRand);
			state.gravity = gravity;

			/*(*addParticle)(particleManager,
				sp_particleRenderTypeStandard,
				&state);*/
		}
	}

		break;
	}

	if(!removeImmediately)
	{
		for(int i = 0; i < 4; i++)
		{
			emitterState->counters[i] = 0;
		}
		emitterState->timeAccumulatorA = 0.0;
		emitterState->timeAccumulatorB = 0.0;
	}

	return removeImmediately;
}

void emitFireParticle(SPParticleThreadState* threadState,
	SPParticleEmitterState* emitterState,
	double scaleAverage,
	SPVec3 randPosVec)
{
	SPRand* spRand = threadState->spRand;
	SPParticleState state;

	double posLength = spVec3Length(emitterState->p);
	SPVec3 normalizedPos = spVec3Div(emitterState->p, posLength);

	state.p = spVec3Add(spVec3Mul(normalizedPos, posLength + SP_METERS_TO_PRERENDER(0.1)), randPosVec);
	state.v = spVec3Mul(normalizedPos, SP_METERS_TO_PRERENDER(0.2 + spRandGetValue(spRand) * 0.2));
	state.particleTextureType = ((spRandGetValue(spRand) > 0.5) ? 1 : 4);
	state.lifeLeft = 1.0;
	state.scale = scaleAverage + spRandGetValue(spRand) * 0.2;
	state.randomValueA = spRandGetValue(spRand);
	state.randomValueB = spRandGetValue(spRand);

	(*threadState->addParticle)(threadState->particleManager,
		sp_particleRenderTypeFire,
		&state);
}

static const double fixedTimeStep = 1.0/60.0;

void spUpdateEmitter(SPParticleThreadState* threadState,
	SPParticleEmitterState* emitterState,
	double dt)
{
	SPRand* spRand = threadState->spRand;

	emitterState->timeAccumulatorA += dt;

	while(emitterState->timeAccumulatorA > 0.0)
	{
		emitterState->timeAccumulatorA -= fixedTimeStep;
		emitterState->timeAccumulatorB += fixedTimeStep;
		switch(emitterState->type)
		{
		case sp_emitterTypeCampfire:
		{
			if(emitterState->counters[0] == 0) // SMOKE
			{
				double posLength = spVec3Length(emitterState->p);
				SPVec3 normalizedPos = spVec3Div(emitterState->p, posLength);
				SPParticleState state;

				state.p = spVec3Mul(normalizedPos, posLength + SP_METERS_TO_PRERENDER(0.2));

				state.particleTextureType = 2;
				state.lifeLeft = 1.0;
				state.scale = 0.2 + spRandGetValue(spRand) * 0.2;
				state.randomValueA = spRandGetValue(spRand);
				//state.gravity = spVec3Mul(spMat3GetRow(emitterState->rot, 0), SP_METERS_TO_PRERENDER(-0.1 + 0.2 * spRandGetValue(spRand)));
				//state.gravity = spVec3Add(state.gravity, spVec3Mul(spMat3GetRow(emitterState->rot, 2), SP_METERS_TO_PRERENDER(-0.1 + 0.2 * spRandGetValue(spRand))));
				SPVec3 lookup = {(normalizedPos.x + 1.2) * 99999.9, (normalizedPos.y * 4.5 + normalizedPos.z + 2.4) * 99999.9, emitterState->timeAccumulatorB * 0.1};
				SPVec3 lookupB = {(normalizedPos.x + 1.4) * 99999.9, (normalizedPos.y * 4.6 + normalizedPos.z + 2.8) * 99999.9, emitterState->timeAccumulatorB * 0.1};
				SPVec3 lookupC = {(normalizedPos.x + 1.8) * 99999.9, (normalizedPos.y * 4.8 + normalizedPos.z + 2.9) * 99999.9, emitterState->timeAccumulatorB * 0.5};
				double noiseValue = spNoiseGet(threadState->spNoise, lookup, 2);
				double noiseValueB = spNoiseGet(threadState->spNoise, lookupB, 2);
				double noiseValueC = spNoiseGet(threadState->spNoise, lookupC, 2);

				state.v = spVec3Mul(normalizedPos, SP_METERS_TO_PRERENDER(0.5 + noiseValueC) * 0.5);

				state.gravity = spVec3Mul(spMat3GetRow(emitterState->rot, 0), SP_METERS_TO_PRERENDER(noiseValue) * 0.5);
				state.gravity = spVec3Add(state.gravity, spVec3Mul(spMat3GetRow(emitterState->rot, 2), SP_METERS_TO_PRERENDER(noiseValueB) * 0.5));

				(*threadState->addParticle)(threadState->particleManager,
					sp_particleRenderTypeSmoke,
					&state);

				emitterState->counters[0] = 1 + (uint8_t)(20 * (1.0 - noiseValueC));
			}
			else
			{
				emitterState->counters[0]--;
			}

			if(emitterState->counters[1] == 0) //FLAME 1
			{
				SPVec3 randPosVec = spVec3Mul(spRandGetVec3(spRand), SP_METERS_TO_PRERENDER(0.04));
				double scaleAverage = 0.5;

				emitFireParticle(threadState,
					emitterState,
					scaleAverage,
					randPosVec);

				emitterState->counters[1] = 5 + (uint8_t)(20 * spRandGetValue(spRand));

			}
			else
			{
				emitterState->counters[1]--;
			}

			if(emitterState->counters[2] == 0) //FLAME 2
			{
				SPVec3 randPosVec = spVec3Mul(spRandGetVec3(spRand), SP_METERS_TO_PRERENDER(0.04));
				randPosVec = spVec3Add(randPosVec, spVec3Mul(spMat3GetRow(emitterState->rot, 0), SP_METERS_TO_PRERENDER(0.2)));
				double scaleAverage = 0.35;

				emitFireParticle(threadState, 
					emitterState,
					scaleAverage,
					randPosVec);

				emitterState->counters[2] = 5 + (uint8_t)(20 * spRandGetValue(spRand));

			}
			else
			{
				emitterState->counters[2]--;
			}

			if(emitterState->counters[3] == 0) //FLAME 3
			{
				SPVec3 randPosVec = spVec3Mul(spRandGetVec3(spRand), SP_METERS_TO_PRERENDER(0.04));
				randPosVec = spVec3Add(randPosVec, spVec3Mul(spMat3GetRow(emitterState->rot, 2), SP_METERS_TO_PRERENDER(0.1)));
				double scaleAverage = 0.2;

				emitFireParticle(threadState, 
					emitterState,
					scaleAverage,
					randPosVec);

				emitterState->counters[3] = 5 + (uint8_t)(20 * spRandGetValue(spRand));
			}
			else 
			{
				if(emitterState->counters[3] == 18) //spark
				{

					double posLength = spVec3Length(emitterState->p);
					SPVec3 normalizedPos = spVec3Div(emitterState->p, posLength);
					SPVec3 randVec = spRandGetVec3(spRand);
					SPVec3 randPosVec = spVec3Mul(randVec, SP_METERS_TO_PRERENDER(0.1));
					SPVec3 randVelVec = spVec3Mul(randVec, SP_METERS_TO_PRERENDER(0.4));

					SPParticleState state;

					state.p = spVec3Add(spVec3Mul(normalizedPos, posLength + SP_METERS_TO_PRERENDER(0.1)), randPosVec);
					state.v = spVec3Mul(spVec3Add(normalizedPos, randVelVec), SP_METERS_TO_PRERENDER(2.0 + spRandGetValue(spRand) * 0.5));
					state.particleTextureType = 3;
					state.lifeLeft = 1.0;
					state.scale = 0.01 + spRandGetValue(spRand) * 0.02;
					state.randomValueA = spRandGetValue(spRand);
					state.randomValueB = spRandGetValue(spRand);
					state.gravity = spVec3Mul(spRandGetVec3(spRand), SP_METERS_TO_PRERENDER(1.0));

					(*threadState->addParticle)(threadState->particleManager,
						sp_particleRenderTypeSpark,
						&state);
				}
				emitterState->counters[3]--;
			}

		}
		break;
		}
	}
}

static const SPVec2 texCoords[4] = {
	{0.0,0.0},
	{0.0,1.0},
	{1.0,1.0},
	{1.0,0.0},
};


bool spUpdateParticle(SPParticleThreadState* threadState, 
	SPParticleState* particleState, 
	uint32_t renderGroup,
	double dt, 
	SPVec3 origin, 
	float* renderBuffer)
{
	double lifeLeftMultiplier = 1.0;

	if(renderGroup == sp_particleRenderTypeSmoke)
	{
		lifeLeftMultiplier = 0.05;
	}
	else if(renderGroup == sp_particleRenderTypeFire)
	{
		lifeLeftMultiplier = (1.5 - particleState->randomValueB * 0.5);
	}
	else if(renderGroup == sp_particleRenderTypeSpark)
	{
		lifeLeftMultiplier = (1.5 - particleState->randomValueB * 1.0);
	}

	double lifeLeft = particleState->lifeLeft - dt * lifeLeftMultiplier;


	particleState->lifeLeft = lifeLeft;
	if(lifeLeft < 0.0)
	{
		return false;
	}

	if(renderGroup == sp_particleRenderTypeFire)
	{

		particleState->p = spVec3Add(particleState->p, spVec3Mul(particleState->v, (2.0 - lifeLeft) * dt));
	}
	else if(renderGroup == sp_particleRenderTypeSmoke)
	{
		particleState->v = spVec3Mul(particleState->v, 1.0 - dt * 0.05);

		SPVec3 vel = spVec3Add(particleState->v, particleState->gravity); //gravity is wind

		particleState->p = spVec3Add(particleState->p, spVec3Mul(vel, dt));

		particleState->scale = particleState->scale + dt * particleState->lifeLeft * (1.0 + particleState->randomValueA) * 0.15;
	}
	else
	{
		particleState->v = spVec3Add(particleState->v, spVec3Mul(particleState->gravity, dt));
		particleState->p = spVec3Add(particleState->p, spVec3Mul(particleState->v, dt));
	}


	SPVec3 posLocal = spVec3Mul(spVec3Sub(particleState->p, origin), SP_RENDER_SCALE);
	for(int v = 0; v < 4; v++)
	{
		renderBuffer[v * 9 + 0] = posLocal.x;
		renderBuffer[v * 9 + 1] = posLocal.y;
		renderBuffer[v * 9 + 2] = posLocal.z;
		renderBuffer[v * 9 + 3] = texCoords[v].x;
		renderBuffer[v * 9 + 4] = texCoords[v].y;
		renderBuffer[v * 9 + 5] = particleState->particleTextureType;
		renderBuffer[v * 9 + 6] = particleState->lifeLeft;
		renderBuffer[v * 9 + 7] = particleState->randomValueA;
		renderBuffer[v * 9 + 8] = particleState->scale;
	}
		
	return true;
}