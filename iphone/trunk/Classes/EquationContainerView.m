//
//  EquationContainerView.m
//  mathomatic
//
//  Created by Ben Gotow on 11/14/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "EquationContainerView.h"
#import "EquationDivisionView.h"

@implementation EquationContainerView

@synthesize innerEquations;
@synthesize parent;


- (id)initWithParent:(EquationContainerView*)p
{
    if (self = [super initWithFrame:CGRectZero]) {
        // Initialization code
        
        self.innerEquations = [NSMutableArray array];
        self.backgroundColor = [UIColor clearColor];
        self.clipsToBounds = YES;
        
        innerEquationsMax = -1;
        self.parent = p;
    }
    return self;
}

- (void)setShowOpenParenthesis:(BOOL)showOpen
{
    if (showOpen){
        openParen = [[UILabel alloc] initWithFrame: CGRectZero];
        [openParen setBackgroundColor: [UIColor clearColor]];
        [self addSubview: openParen];
        [openParen setText: @"("];
    }else{
        [openParen removeFromSuperview];
        [openParen release];
        openParen = nil;
    }
}

- (void)setShowCloseParenthesis:(BOOL)showClose
{
    if (showClose){
        closeParen = [[UILabel alloc] initWithFrame: CGRectZero];
        [closeParen setBackgroundColor: [UIColor clearColor]];
        [self addSubview: closeParen];
        [closeParen setText: @")"];
    }else{
        [closeParen removeFromSuperview];
        [closeParen release];
        closeParen = nil;
    }
}

- (EquationAbstractView*)popInnerEquation
{
    EquationAbstractView * view = [[[self.innerEquations lastObject] retain] autorelease];
    [self.innerEquations removeObjectIdenticalTo: view];
    [view removeFromSuperview];
    return view;
}

- (BOOL)addInnerEquation:(EquationAbstractView*)equation
{
    if ((innerEquationsMax == -1) || ([self.innerEquations count] < innerEquationsMax)){
        [self.innerEquations addObject: equation];
        [self addSubview: equation];
    }else
        return NO;
    return YES;
}

- (BOOL)innerEquationIsFull
{
    if ([self.innerEquations count] < innerEquationsMax)
        return NO;
    else
        return YES;
}

- (void)finalizeEquationHierarchy
{
    BOOL containsDivision = NO;
    for (int ii = [self.innerEquations count] - 1; ii >= 0; ii --){
        if ([[[innerEquations objectAtIndex: ii] class] isEqual: [EquationDivisionView class]])
            containsDivision = YES;
        [[innerEquations objectAtIndex: ii] finalizeEquationHierarchy];
    }
        
    if (containsDivision)
        alignmentMethod = kAlignmentMethodCentered;
    else if ([innerEquations count] == 1)
        alignmentMethod = [[innerEquations lastObject] alignmentMethod];
    else
        alignmentMethod = kAlignmentMethodBottom;
}

- (void)finalizeTextSize:(int)parentTextSize
{
    for (EquationAbstractView * v in self.innerEquations)
        [v finalizeTextSize: parentTextSize];
    
    [openParen setFont: [UIFont fontWithName:@"Courier" size:parentTextSize]];
    [closeParen setFont: [UIFont fontWithName:@"Courier" size:parentTextSize]];
}

- (void)finalizeFrame
{
    [self.innerEquations makeObjectsPerformSelector:@selector(finalizeFrame)];
    
    int heightAboveBaseline = 0;
    int heightBelowBaseline = 0;
    int x = 0;
    
    for (EquationAbstractView * v in self.innerEquations){
        if (v.alignmentMethod == kAlignmentMethodBottom){
            heightAboveBaseline = fmax([v frame].origin.y + [v frame].size.height, heightAboveBaseline);
        } else if (v.alignmentMethod == kAlignmentMethodCentered){
            heightAboveBaseline = fmax(([v frame].origin.y + [v frame].size.height)/2, heightAboveBaseline);
            heightBelowBaseline = fmax(([v frame].origin.y + [v frame].size.height)/2, heightBelowBaseline);
        }
    }
    
    CGSize openSize = [[openParen text] sizeWithFont: [openParen font]];
    CGSize closeSize = [[closeParen text] sizeWithFont: [closeParen font]];
    
    x = openSize.width;
    
    for (EquationAbstractView * v in self.innerEquations){
        int offset;
        if (v.alignmentMethod == kAlignmentMethodBottom)
            offset = (heightBelowBaseline)/2 + heightAboveBaseline-[v frame].size.height;
        else
            offset = heightAboveBaseline - ([v frame].size.height) / 2;
        [v setFrame: CGRectMake(x, offset, [v frame].size.width, [v frame].size.height)];
        x += [v frame].size.width;
    }
    
    int height = fmax(heightAboveBaseline + heightBelowBaseline, openSize.height);

    [openParen setFrame: CGRectMake(0, (heightAboveBaseline + heightBelowBaseline - openSize.height)/2, openSize.width, openSize.height)];
    [closeParen setFrame: CGRectMake(x, (heightAboveBaseline + heightBelowBaseline - closeSize.height)/2, closeSize.width, closeSize.height)];
    [self setFrame: CGRectMake(0,0, x + closeSize.width, height)];
}

- (NSString*)description
{
    NSMutableString * description = [NSMutableString stringWithString: @"("];
    for (EquationAbstractView * view in self.innerEquations)
        [description appendString: [view description]];
    [description appendFormat: @") [frame=%f,%f,%f,%f]", [self frame].origin.x, [self frame].origin.y, [self frame].size.width, [self frame].size.height];
    return description;
}

- (void)dealloc {
    [openParen release];
    [closeParen release];
    [super dealloc];
}


@end
