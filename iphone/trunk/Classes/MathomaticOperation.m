//
//  MathomaticOperation.m
//  mathomatic
//
//  Created by Ben Gotow on 11/20/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "MathomaticOperation.h"


@implementation MathomaticOperation
@synthesize inputs;
@synthesize steps;
@synthesize stepNames;
@synthesize outputs;
@synthesize name;

- (id)init
{
    if (self = [super init]){
        inputs = [[NSMutableArray alloc] init];
        outputs = [[NSMutableArray alloc] init];
        steps = [[NSMutableArray alloc] init];
        stepNames = [[NSMutableArray alloc] init];
    }
    return self;
}

- (void)addInput:(MathomaticExpression*)expression
{
    [inputs addObject: expression];
}

- (void)addStep:(MathomaticExpression*)expression withName:(NSString*)n
{
    [steps addObject: expression];
    [stepNames addObject: n];
}

- (void)performStart
{
    matho_init();
    [steps removeAllObjects];
    [stepNames removeAllObjects];
    [outputs removeAllObjects];
}

- (NSString*)performExpression:(MathomaticExpression*)i
{
    char * buffer = NULL;
    matho_process([[i mathomaticText] cStringUsingEncoding: NSStringEncodingConversionAllowLossy], &buffer);
    if (buffer != NULL) 
        return [NSString stringWithCString: buffer];
    else
        return nil;
}

- (void)perform
{
    for (i in input)
        [self performExpression: i];
}

@end
