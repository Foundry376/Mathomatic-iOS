//
//  OperationIntegrate.m
//  mathomatic
//
//  Created by Ben Gotow on 10/25/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "OperationFactor.h"


@implementation OperationFactor

- (void)run
{
    MathCommand * c = [[MathCommand alloc] init];
    [c addInput: entry];
    [c addInput: @"factor"];
    [delegate performCommand: c];
    [c release];
    [self release];
}

@end
