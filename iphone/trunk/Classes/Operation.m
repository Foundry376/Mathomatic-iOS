//
//  Operation.m
//  mathomatic
//
//  Created by Ben Gotow on 10/25/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "Operation.h"
#import "MathomaticViewController.h"

@implementation Operation

@synthesize delegate;
@synthesize entry;

- (id)initWithDelegate: (MathomaticViewController*)d andEntry:(NSString*)e
{
    if (self = [super init]){
        self.delegate = d;
        self.entry = e;
    }
    return self;
}

- (void)dealloc
{
    [entry release];
    [delegate release];
    [super dealloc];
}

- (void)run
{
}

@end
